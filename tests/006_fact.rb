# Copyright 2013 Jarek Jarcec Cecho <jarcec@apache.org>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
require 'test/unit'
require './rbclips'

class Test_Fact < Test::Unit::TestCase

  def teardown
    Clips::Base.reset
  end

  def test_exists
    assert Clips.constants.member?(:Fact)
  end

  def get_animal
    Clips::Template.new 'animal' do |t| 
      t.slot :name
      t.slot :age
      t.slot :race
    end
  end

  def test_ordered
    assert_nothing_raised   { Clips::Fact.new 'human', [:age, 'name', 30, 1.5] }
    assert_nothing_raised   { Clips::Fact.new 'human', %w(jedna dva tri ctyri) }

    a = Clips::Fact.new 'human', [:jarcec, 22]
    assert_equal a.instance_eval { @slots }, [:jarcec, 22]
    assert_equal a.instance_eval { @template }, 'human'

    assert a.save
    assert a.save
    assert a.destroy!
    assert !a.destroy!

    assert a.ordered?
  end

  def test_nonordered
    template = get_animal

    assert_raise(Clips::ArgumentError)  { Clips::Fact.new template, :name  => 'agatha' }
    assert template.save
    assert_nothing_raised               { Clips::Fact.new template, :name  => 'agatha' }
    assert_nothing_raised               { Clips::Fact.new template, 'name' => 'agatha' }
    assert_raise(Clips::ArgumentError)  { Clips::Fact.new template, :years => 'agatha' }
    assert_raise(Clips::ArgumentError)  { Clips::Fact.new template, 29     => 'agatha' }

    a = Clips::Fact.new template, :name => 'agatha', 'age' => 30
    assert_equal a.instance_eval { @slots }, { :name => 'agatha', :age => 30, :race => nil}
    assert_equal a.instance_eval { @template }, template

    assert !a.ordered?
    assert template.destroy!
  end

  def test_to_s
    c = Clips::Fact.new 'human', %w(jarek 30)
    assert_equal c.to_s, '(human "jarek" "30")'

    template = get_animal
    assert template.save
    
    c = Clips::Fact.new template, :name => 'ales', :age => 30, 'race' => 'pes'
    assert_equal c.to_s, '(animal (name "ales") (age 30) (race "pes"))'

    c = Clips::Fact.new template, :name => 'ales', :race => 'pes'
    assert_equal c.to_s, '(animal (name "ales") (race "pes"))'

    # Array ~= multifield
    c = Clips::Fact.new template, :name => 'ales', :race => %w(pes dog)
    assert_equal c.to_s, '(animal (name "ales") (race "pes" "dog"))'

    assert template.destroy!
  end

  def test_clone
    a = Clips::Fact.new 'human', %w(x y z)
    b = a.clone
    c = b.dup

    assert a.eql? b
    assert b.equal? c
    assert a == c

    a.save
    assert ! ( a.eql? b )
    assert ! ( a.equal? b )
    assert ! ( a == b )
    assert b == c
  end

  def test_slots
    a = Clips::Fact.new 'human', %w(x y z)
    assert a.slots
    assert_equal a.slots, ['x', 'y', 'z']

    a = Clips::Fact.new 'human', [30, :ahoj, 'zdar', 4.2]
    assert_equal a.slots, [30, :ahoj, 'zdar', 4.2]
  end

  def test_slot
    template = get_animal
    assert template.save

    a = Clips::Fact.new template, :name => "Bruno", :age => 30
    assert_equal a.slot(:name), "Bruno"
    assert_equal a.slot(:age), 30
    assert_equal a.slot(:race), nil
    assert_raise(Clips::ArgumentError) { a.slot(:bulb) }
    assert_raise(Clips::ArgumentError) { a.slot(30) }
    assert_raise(Clips::ArgumentError) { a.slot(2.4) }

    assert_equal a.name, "Bruno"
    assert_equal a.age, 30
    assert_equal a.race, nil

    assert template.destroy!
  end

  def test_name
    # Ordered variant
    a = Clips::Fact.new "human", %w(jedna dve tri)
    assert_equal a.name, "human"
    
    # Nonordered variant
    template = get_animal
    assert template.save

    a = Clips::Fact.new template, :name => "Bruno", :age => 30
    assert_equal a.template, template

    assert template.destroy!
  end

  def test_all
    template = get_animal
    assert template.save

    a = Clips::Fact.new template, :name => 'Azor'
    b = Clips::Fact.new template, :age => 30
    c = Clips::Fact.new 'windows', %w(I don't like them)

    assert a.save
    assert b.save
    assert c.save

    assert_equal Clips::Fact.all, [a, b, c]

    assert a.destroy!
    assert b.destroy!
    assert c.destroy!

    assert template.destroy!
  end

  def test_find
    template = get_animal
    assert template.save
    
    assert Clips::Fact.new('os', %w(linux)).save
    assert Clips::Fact.new('os', %w(MacOS)).save
    assert Clips::Fact.new('os', %w(FreeBsd)).save
    assert Clips::Fact.new('gameloader', %w(Windows)).save
    assert Clips::Fact.new(template, :name => 'alik').save
    assert Clips::Fact.new(template, :name => 'zorek').save
 
    assert_equal Clips::Fact.find('os').size, 3
    assert_equal Clips::Fact.find('gameloader').size, 1
    assert_equal Clips::Fact.find(template).size, 2
    assert_equal Clips::Fact.find('maminka').size, 0

    Clips::Fact.all.each {|f| assert f.destroy! }
    assert template.destroy!
  end

  def test_instance_methods_ordered
    assert Clips::Fact.new('gameloader', %w(windows)).save

    assert_not_nil a = Clips::Fact.find('gameloader').first
    assert_respond_to a, :name
    assert_respond_to a, :slots

    assert_not_nil  = Clips::Fact.all.first
    assert_respond_to a, :name
    assert_respond_to a, :slots
  end

  def test_instance_methods_nonordered
    template = get_animal
    assert template.save

    assert Clips::Fact.new(template, :name => "Azor", :race => "dog", :age => 30).save

    assert_not_nil a = Clips::Fact.find(template).first
    assert_respond_to a, :template
    assert_respond_to a, :slot
    assert_respond_to a, :name
    assert_respond_to a, :race
    assert_respond_to a, :age

    assert_not_nil  = Clips::Fact.all.first
    assert_respond_to a, :template
    assert_respond_to a, :slot
    assert_respond_to a, :name
    assert_respond_to a, :race
    assert_respond_to a, :age
  
    assert a.destroy!
    assert template.destroy!
  end

  def test_saved
    t = Clips::Template.new('animal') do |t|
      t.slot :name
      t.slot :age
      t.slot :race
    end
    t.save

    a = Clips::Fact.new(t, :name => "Azor")
    assert !a.saved?

    assert a.save
    assert a.saved?

    a.name = "Bohus"
    assert !a.saved?

    a.save
    assert a.saved?
  end

  def test_save_update
    t = Clips::Template.new('animal') do |t|
      t.slot :name
      t.slot :age
      t.slot :race
    end
    assert t.save

    a = Clips::Fact.new(t, :name => "Azor")

    assert a.save
    b = Clips::Fact.find(t).first;
    assert_equal a.name, "Azor"
    assert_equal b.name, "Azor"

    assert a.name = "Bohus"
    b = Clips::Fact.find(t).first
    assert_equal a.name, "Bohus"
    assert_equal b.name, "Azor"

    a.save
    b = Clips::Fact.find(t).first
    assert_equal a.name, "Bohus"
    assert_equal b.name, "Bohus"
  end
end
